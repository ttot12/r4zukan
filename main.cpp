/*
 * R4 黑白图鉴菜单 · 原创 NDS 启动器
 * 风格致敬《宝可梦黑白2》图鉴界面（结构/布局致敬，代码与素材均为原创，不含任天堂版权代码/素材）
 * 目标：编译成 .nds 放进 R4 卡运行
 *
 * 布局：
 *   上屏：左侧红框=游戏目录列表 | 右侧=游戏封面(程序绘制) | 下方蓝框=游戏信息
 *   下屏：4 格 R4 菜单(游戏/设置/菜单/帮助) + 右下角宠物精灵轮播
 * 交互：方向键上下移动游戏列表光标，A 选中；下屏触屏点击切换菜单视图
 */
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== 颜色 (RGB15: r,g,b 各 0-31) ===== */
#define C_BG     RGB15(8,9,13)      // 背景深蓝灰
#define C_PANEL  RGB15(11,13,19)    // 面板
#define C_PANEL2 RGB15(13,16,23)    // 面板高亮
#define C_LINE   RGB15(17,20,27)    // 线框
#define C_TXT    RGB15(30,31,31)    // 文字白
#define C_MUTE   RGB15(16,19,26)    // 次要文字
#define C_RED    RGB15(30,18,15)    // 红框
#define C_BLUE   RGB15(14,23,31)    // 蓝框
#define C_ACC    RGB15(18,26,31)    // 强调蓝

u16* topFb = (u16*)0x06000000;      // 上屏(MAIN)位图
u16* subFb = (u16*)0x06200000;      // 下屏(SUB)位图
#define FB_W 256
#define FB_H 192

/* ===== 基础绘制 ===== */
static inline void px(u16* fb,int x,int y,u16 c){
    if(x<0||x>=FB_W||y<0||y>=FB_H) return;
    fb[x + y*FB_W] = c;
}
static void fillRect(u16* fb,int x,int y,int w,int h,u16 c){
    for(int j=0;j<h;j++)
        for(int i=0;i<w;i++) px(fb,x+i,y+j,c);
}
static void frameRect(u16* fb,int x,int y,int w,int h,int t,u16 c){
    for(int i=0;i<w;i++){ for(int k=0;k<t;k++){ px(fb,x+i,y+k,c); px(fb,x+i,y+h-1-k,c);} }
    for(int j=t;j<h-t;j++){ for(int k=0;k<t;k++){ px(fb,x+k,y+j,c); px(fb,x+w-1-k,y+j,c);} }
}

/* ===== 5x7 点阵字体（ASCII 子集） ===== */
static const uint8_t* glyph(char c){
    static const uint8_t g0[7]={0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}; // 0
    static const uint8_t g1[7]={0x04,0x0C,0x04,0x04,0x04,0x04,0x0E};
    static const uint8_t g2[7]={0x0E,0x11,0x01,0x02,0x04,0x08,0x1F};
    static const uint8_t g3[7]={0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E};
    static const uint8_t g4[7]={0x02,0x06,0x0A,0x12,0x1F,0x02,0x02};
    static const uint8_t g5[7]={0x1F,0x10,0x1E,0x01,0x01,0x01,0x1E};
    static const uint8_t g6[7]={0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E};
    static const uint8_t g7[7]={0x1F,0x01,0x02,0x04,0x08,0x08,0x08};
    static const uint8_t g8[7]={0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E};
    static const uint8_t g9[7]={0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E};
    static const uint8_t gA[7]={0x0E,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const uint8_t gB[7]={0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
    static const uint8_t gC[7]={0x0E,0x11,0x10,0x10,0x10,0x11,0x0E};
    static const uint8_t gD[7]={0x1C,0x12,0x11,0x11,0x11,0x12,0x1C};
    static const uint8_t gE[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F};
    static const uint8_t gF[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x10};
    static const uint8_t gG[7]={0x0E,0x11,0x10,0x17,0x11,0x11,0x0F};
    static const uint8_t gH[7]={0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const uint8_t gI[7]={0x0E,0x04,0x04,0x04,0x04,0x04,0x0E};
    static const uint8_t gJ[7]={0x07,0x02,0x02,0x02,0x02,0x12,0x0C};
    static const uint8_t gK[7]={0x11,0x12,0x14,0x18,0x14,0x12,0x11};
    static const uint8_t gL[7]={0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
    static const uint8_t gM[7]={0x11,0x1B,0x15,0x15,0x11,0x11,0x11};
    static const uint8_t gN[7]={0x11,0x19,0x15,0x13,0x11,0x11,0x11};
    static const uint8_t gO[7]={0x0E,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const uint8_t gP[7]={0x1E,0x11,0x11,0x1E,0x10,0x10,0x10};
    static const uint8_t gQ[7]={0x0E,0x11,0x11,0x11,0x15,0x12,0x0D};
    static const uint8_t gR[7]={0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
    static const uint8_t gS[7]={0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E};
    static const uint8_t gT[7]={0x1F,0x04,0x04,0x04,0x04,0x04,0x04};
    static const uint8_t gU[7]={0x11,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const uint8_t gV[7]={0x11,0x11,0x11,0x11,0x11,0x0A,0x04};
    static const uint8_t gW[7]={0x11,0x11,0x11,0x15,0x15,0x1B,0x11};
    static const uint8_t gX[7]={0x11,0x11,0x0A,0x04,0x0A,0x11,0x11};
    static const uint8_t gY[7]={0x11,0x11,0x0A,0x04,0x04,0x04,0x04};
    static const uint8_t gZ[7]={0x1F,0x01,0x02,0x04,0x08,0x10,0x1F};
    static const uint8_t gAnd[7]={0x0E,0x11,0x10,0x0C,0x12,0x11,0x0F};
    static const uint8_t gMinus[7]={0,0,0,0x1F,0,0,0};
    static const uint8_t gDot[7]={0,0,0,0,0,0x0C,0x0C};
    static const uint8_t gSlash[7]={0x01,0x02,0x02,0x04,0x08,0x08,0x10};
    static const uint8_t gColon[7]={0,0x0C,0x0C,0,0x0C,0x0C,0};
    static const uint8_t gSpace[7]={0,0,0,0,0,0,0};
    switch(c){
        case ' ': return gSpace; case '0': return g0; case '1': return g1; case '2': return g2;
        case '3': return g3; case '4': return g4; case '5': return g5; case '6': return g6;
        case '7': return g7; case '8': return g8; case '9': return g9; case 'A': return gA;
        case 'B': return gB; case 'C': return gC; case 'D': return gD; case 'E': return gE;
        case 'F': return gF; case 'G': return gG; case 'H': return gH; case 'I': return gI;
        case 'J': return gJ; case 'K': return gK; case 'L': return gL; case 'M': return gM;
        case 'N': return gN; case 'O': return gO; case 'P': return gP; case 'Q': return gQ;
        case 'R': return gR; case 'S': return gS; case 'T': return gT; case 'U': return gU;
        case 'V': return gV; case 'W': return gW; case 'X': return gX; case 'Y': return gY;
        case 'Z': return gZ; case '&': return gAnd; case '-': return gMinus;
        case '.': return gDot; case '/': return gSlash; case ':': return gColon;
        default: return gSpace;
    }
}
static void drawText(u16* fb,int x,int y,const char* s,u16 c){
    int cx=x;
    for(const char* p=s; *p; p++){
        const uint8_t* g=glyph(*p);
        for(int row=0; row<7; row++){
            uint8_t bits=g[row];
            for(int col=0; col<5; col++){
                if(bits&(1<<col)) px(fb, cx+col, y+row, c);
            }
        }
        cx += 6;
    }
}

/* ===== 原创像素宠物（右下角轮播） ===== */
typedef struct { const char* name; int lv; const char* rows[8]; } Pet;
static const Pet pets[4] = {
    { "FLAMIT", 18, { "..RRRR..",".RORROR.",".RRRRRR.",".RKWWKR.",".RKKKKR.","..RRRR..","...RR...","...RR..." } },
    { "WAVICLAW",20,{ "..BBBB..",".BDBBDB.",".BBBBBB.",".BWBBWB.",".BKKKKB.","..BKKB..","..BKKB..","..BKKB.." } },
    { "THORN", 16, { "...GG...","..GLGL..",".GGGGGG.",".GKWWKG.",".GGKKGG.",".LGKKGL.",".L.LL.L.","..L..L.." } },
    { "SPARK", 17, { ".YY..YY.",".YY..YY.",".YYYYYY.",".YEWWEY.",".YYKKYY.",".YYYYYY.","..YYYY..","...YY..." } }
};
static void drawPet(u16* fb,int x,int y,const Pet* p,u16 fillC,u16 shC,u16 hlC){
    const char* const* rows = p->rows;
    int hx=0, hy=0;
    for(int j=0;j<8;j++){
        for(int i=0;i<8;i++){
            char ch=rows[j][i];
            if(ch=='.') continue;
            u16 c;
            switch(ch){ case 'R': c=RGB15(31,20,10); break; case 'O': c=RGB15(31,28,20); break;
                case 'B': c=RGB15(12,24,31); break; case 'D': c=RGB15(6,16,26); break;
                case 'G': c=RGB15(10,27,16); break; case 'L': c=RGB15(20,31,22); break;
                case 'Y': c=RGB15(31,28,12); break; case 'E': c=RGB15(31,31,20); break;
                case 'W': c=RGB15(30,31,31); break; case 'K': c=RGB15(6,6,9); break;
                default: c=C_TXT; }
            px(fb, x+i, y+j, c);
        }
    }
}

/* ===== 游戏目录数据 ===== */
typedef struct { const char* name; const char* type; const char* size; const char* desc; } Game;
static const Game games[6] = {
    { "DSCRAFT", "SANDBOX", "0.6MB", "BLOCK WORLD" },
    { "PORTALDS","PUZZLE", "2.2MB", "PORTAL PUZZLE" },
    { "NINJI",  "ACTION", "3.0MB", "2P ACTION" },
    { "GBA EMU", "EMULATOR","1.1MB","GBA PLAYER" },
    { "FC EMU", "EMULATOR","0.9MB","8-BIT PLAYER" },
    { "STARSHOT","SHOOTER","1.4MB","SPACE SHOOTER" }
};
#define NGAME 6

/* ===== 状态 ===== */
enum View { V_GAME, V_SET, V_MENU, V_HELP };
static int curView=V_GAME;
static int curSel=0;
static int petIdx=0;
static int frames=0;

/* ===== 上屏：游戏视图 ===== */
static void drawCover(u16* fb,int x,int y,int w,int h,int idx){
    // 原创占位封面：按游戏画一个简单图案 + 色块
    u16 base = (idx%2)? RGB15(6,18,26) : RGB15(24,10,8);
    fillRect(fb,x,y,w,h,base);
    int cx=x+w/2, cy=y+h/2;
    for(int r=4; r<24; r+=4){
        frameRect(fb, cx-r, cy-r, r*2+1, r*2+1, 1, (r%8)? C_BLUE : C_RED);
    }
    drawText(fb, x+6, y+h-10, "COVER", C_TXT);
}
static void renderTopGame(){
    // 左侧红框：游戏目录
    frameRect(topFb,4,20,92,120,2,C_RED);
    fillRect(topFb,5,21,90,118,C_PANEL);
    drawText(topFb,8,24,"GAME LIST",C_RED);
    for(int i=0;i<NGAME;i++){
        int yy=36+i*14;
        if(i==curSel){ fillRect(topFb,7,yy-1,86,12,C_RED); drawText(topFb,10,yy,games[i].name,C_TXT); }
        else drawText(topFb,10,yy,games[i].name,C_MUTE);
    }
    // 右侧：封面
    int covX=102, covY=20, covW=150, covH=96;
    frameRect(topFb,covX,covY,covW,covH,1,C_LINE);
    drawCover(topFb,covX+1,covY+1,covW-2,covH-2,curSel);
    drawText(topFb,covX+4,covY+covH-9, games[curSel].name, C_TXT);
    // 下方蓝框：游戏信息
    int infX=102, infY=120, infW=150, infH=66;
    frameRect(topFb,infX,infY,infW,infH,2,C_BLUE);
    fillRect(topFb,infX+1,infY+1,infW-2,infH-2,C_PANEL);
    drawText(topFb,infX+6,infY+4,"INFO",C_BLUE);
    drawText(topFb,infX+6,infY+14,games[curSel].name,C_TXT);
    drawText(topFb,infX+6,infY+26,"TYPE: ",C_MUTE); drawText(topFb,infX+36,infY+26,games[curSel].type,C_ACC);
    drawText(topFb,infX+6,infY+36,"SIZE: ",C_MUTE); drawText(topFb,infX+36,infY+36,games[curSel].size,C_ACC);
    drawText(topFb,infX+6,infY+46,"DESC: ",C_MUTE); drawText(topFb,infX+36,infY+46,games[curSel].desc,C_ACC);
    drawText(topFb,infX+6,infY+56,"[A] LAUNCH",C_RED);
}
static void renderTopSettings(){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"SETTINGS",C_BLUE);
    drawText(topFb,14,44,"LANG   : CN",C_TXT);
    drawText(topFb,14,58,"BRIGHT : 5/10",C_TXT);
    drawText(topFb,14,72,"VOLUME : 80%",C_TXT);
    drawText(topFb,14,86,"THEME  : ZUKAN",C_TXT);
    drawText(topFb,14,100,"CLOCK  : 12:00",C_TXT);
    drawText(topFb,14,124,"(DEMO VALUES)",C_MUTE);
}
static void renderTopMenu(){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"MAIN MENU",C_BLUE);
    drawText(topFb,14,46,"1 GAME",C_TXT);
    drawText(topFb,14,62,"2 SETTINGS",C_TXT);
    drawText(topFb,14,78,"3 MEDIA",C_TXT);
    drawText(topFb,14,94,"4 HELP",C_TXT);
}
static void renderTopHelp(){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"HELP",C_BLUE);
    drawText(topFb,10,44,"UP/DOWN : SELECT",C_TXT);
    drawText(topFb,10,56,"A : ENTER GAME",C_TXT);
    drawText(topFb,10,68,"B : BACK",C_TXT);
    drawText(topFb,10,80,"TOUCH BOTTOM : MENU",C_TXT);
    drawText(topFb,10,96,"ORIGINAL HOMEBREW",C_MUTE);
    drawText(topFb,10,106,"ZUKAN-STYLE R4 MENU",C_MUTE);
}
static void renderTop(){
    fillRect(topFb,0,0,FB_W,FB_H,C_BG);
    drawText(topFb,4,2,"ZUKAN MENU",C_TXT);
    switch(curView){
        case V_GAME: renderTopGame(); break;
        case V_SET:  renderTopSettings(); break;
        case V_MENU: renderTopMenu(); break;
        case V_HELP: renderTopHelp(); break;
    }
}

/* ===== 下屏：R4 菜单 + 宠物轮播 ===== */
static void renderBottom(){
    fillRect(subFb,0,0,FB_W,FB_H,C_BG);
    drawText(subFb,4,2,"R4 MENU",C_TXT);
    // 4 格菜单
    const char* labels[4]={"GAME","SET","MENU","HELP"};
    const char* subs[4]={"BROWSE","OPTIONS","OVERVIEW","USAGE"};
    for(int i=0;i<4;i++){
        int x=(i%2)*124+6, y=16+(i/2)*44;
        int w=118, h=38;
        int active = (curView==i);
        frameRect(subFb,x,y,w,h,1, active? C_ACC : C_LINE);
        fillRect(subFb,x+1,y+1,w-2,h-2, active? C_PANEL2 : C_PANEL);
        drawText(subFb,x+6,y+6,labels[i], active? C_ACC : C_TXT);
        drawText(subFb,x+6,y+18,subs[i],C_MUTE);
    }
    // 右下角宠物轮播
    int px0=178, py0=112;
    frameRect(subFb,px0-2,py0-2,74,74,1,C_LINE);
    fillRect(subFb,px0-1,py0-1,72,72,C_PANEL);
    drawPet(subFb,px0+4,py0+6,&pets[petIdx],C_TXT,C_TXT,C_TXT);
    drawText(subFb,px0+4,py0+58, pets[petIdx].name, C_TXT);
    drawText(subFb,px0+4,py0+66, "LV", C_MUTE);
    // 左下角提示
    drawText(subFb,6,106,"PET CAROUSEL",C_MUTE);
    drawText(subFb,6,132,"1/4 AUTO",C_MUTE);
}

int main(void){
    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);
    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankC(VRAM_C_SUB_BG);   // 下屏位图用 C 显存(128KB,映射0x06200000)；H 只有32KB放不下96KB位图
    REG_BG2CNT   = BG_BMP_BASE(0) | BG_BMP16_256x256;
    REG_BG2CNT_SUB= BG_BMP_BASE(0) | BG_BMP16_256x256;

    touchPosition touch;
    while(1){
        swiWaitForVBlank();
        scanKeys();
        u32 keys = keysHeld();
        u32 down = keysDown();

        if(keys & KEY_UP)   curSel = (curSel-1+NGAME)%NGAME;
        if(keys & KEY_DOWN) curSel = (curSel+1)%NGAME;
        if(down & KEY_A){ if(curView==V_GAME) curSel=curSel; /* 演示：选中游戏 */ }
        if(down & KEY_B) curView=V_GAME;
        // 触屏点击下屏 4 格
        if(down & KEY_TOUCH){
            touchRead(&touch);
            int tx=touch.px, ty=touch.py;
            for(int i=0;i<4;i++){
                int x=(i%2)*124+6, y=16+(i/2)*44;
                if(tx>=x && tx<=x+118 && ty>=y && ty<=y+38){ curView=i; }
            }
        }

        // 宠物轮播：每 ~60 帧换一只
        frames++;
        if(frames>=60){ frames=0; petIdx=(petIdx+1)%4; }

        renderTop();
        renderBottom();
        swiWaitForVBlank();
    }
    return 0;
}
