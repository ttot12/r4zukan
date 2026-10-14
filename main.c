/*
 * R4 黑白图鉴菜单 · 原创 NDS 启动器 (经典兼容版)
 * 纯硬件寄存器驱动，不依赖 libnds —— 生成老 R4 内核 / DeSmuME 0.9.13 都能加载的经典格式
 * 布局：上屏 左红框游戏目录|右封面|下蓝框信息；下屏 4格R4菜单 + 右下宠物轮播
 * 交互：方向键上下选游戏，A/B 切换视图
 */

typedef volatile unsigned short vu16;
typedef volatile unsigned int   vu32;
typedef unsigned short u16;
typedef unsigned int u32;

#define REG_POWERCNT     (*(vu32*)0x04003004)
#define REG_DISPCNT      (*(vu32*)0x04000000)
#define REG_DISPCNT_SUB  (*(vu32*)0x04001000)
#define REG_BG2CNT       (*(vu16*)0x0400000C)
#define REG_BG2CNT_SUB   (*(vu16*)0x0400100C)
#define REG_VRAMCNT_A    (*(vu16*)0x04000240)
#define REG_VRAMCNT_C    (*(vu16*)0x04000244)
#define REG_KEYINPUT     (*(vu16*)0x04000130)
#define REG_VCOUNT       (*(vu16*)0x04000006)

#define RGB15(r,g,b) ((u16)(((r)&31) | (((g)&31)<<5) | (((b)&31)<<10)))

u16* const topFb = (u16*)0x06000000;   /* 上屏(MAIN) 位图 bank A */
u16* const subFb = (u16*)0x06200000;   /* 下屏(SUB)  位图 bank C */
#define FB_W 256
#define FB_H 192

/* ===== 颜色 (RGB15) ===== */
#define C_BG     RGB15(8,9,13)
#define C_PANEL  RGB15(11,13,19)
#define C_PANEL2 RGB15(13,16,23)
#define C_LINE   RGB15(17,20,27)
#define C_TXT    RGB15(30,31,31)
#define C_MUTE   RGB15(16,19,26)
#define C_RED    RGB15(30,18,15)
#define C_BLUE   RGB15(14,23,31)
#define C_ACC    RGB15(18,26,31)

/* ===== 基础绘制 ===== */
static inline void px(u16* fb,int x,int y,u16 c){
    if(x<0||x>=FB_W||y<0||y>=FB_H) return;
    fb[x + y*FB_W] = c;
}
static void fillRect(u16* fb,int x,int y,int w,int h,u16 c){
    int i,j;
    for(j=0;j<h;j++) for(i=0;i<w;i++) px(fb,x+i,y+j,c);
}
static void frameRect(u16* fb,int x,int y,int w,int h,int t,u16 c){
    int i,j,k;
    for(i=0;i<w;i++){ for(k=0;k<t;k++){ px(fb,x+i,y+k,c); px(fb,x+i,y+h-1-k,c);} }
    for(j=t;j<h-t;j++){ for(k=0;k<t;k++){ px(fb,x+k,y+j,c); px(fb,x+w-1-k,y+j,c);} }
}

/* ===== 5x7 点阵字体 (ASCII 子集) ===== */
static const unsigned char* glyph(char c){
    static const unsigned char g0[7]={0x0E,0x11,0x13,0x15,0x19,0x11,0x0E};
    static const unsigned char g1[7]={0x04,0x0C,0x04,0x04,0x04,0x04,0x0E};
    static const unsigned char g2[7]={0x0E,0x11,0x01,0x02,0x04,0x08,0x1F};
    static const unsigned char g3[7]={0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E};
    static const unsigned char g4[7]={0x02,0x06,0x0A,0x12,0x1F,0x02,0x02};
    static const unsigned char g5[7]={0x1F,0x10,0x1E,0x01,0x01,0x01,0x1E};
    static const unsigned char g6[7]={0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E};
    static const unsigned char g7[7]={0x1F,0x01,0x02,0x04,0x08,0x08,0x08};
    static const unsigned char g8[7]={0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E};
    static const unsigned char g9[7]={0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E};
    static const unsigned char gA[7]={0x0E,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const unsigned char gB[7]={0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
    static const unsigned char gC[7]={0x0E,0x11,0x10,0x10,0x10,0x11,0x0E};
    static const unsigned char gD[7]={0x1C,0x12,0x11,0x11,0x11,0x12,0x1C};
    static const unsigned char gE[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F};
    static const unsigned char gF[7]={0x1F,0x10,0x10,0x1E,0x10,0x10,0x10};
    static const unsigned char gG[7]={0x0E,0x11,0x10,0x17,0x11,0x11,0x0F};
    static const unsigned char gH[7]={0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const unsigned char gI[7]={0x0E,0x04,0x04,0x04,0x04,0x04,0x0E};
    static const unsigned char gJ[7]={0x07,0x02,0x02,0x02,0x02,0x12,0x0C};
    static const unsigned char gK[7]={0x11,0x12,0x14,0x18,0x14,0x12,0x11};
    static const unsigned char gL[7]={0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
    static const unsigned char gM[7]={0x11,0x1B,0x15,0x15,0x11,0x11,0x11};
    static const unsigned char gN[7]={0x11,0x19,0x15,0x13,0x11,0x11,0x11};
    static const unsigned char gO[7]={0x0E,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const unsigned char gP[7]={0x1E,0x11,0x11,0x1E,0x10,0x10,0x10};
    static const unsigned char gQ[7]={0x0E,0x11,0x11,0x11,0x15,0x12,0x0D};
    static const unsigned char gR[7]={0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
    static const unsigned char gS[7]={0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E};
    static const unsigned char gT[7]={0x1F,0x04,0x04,0x04,0x04,0x04,0x04};
    static const unsigned char gU[7]={0x11,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const unsigned char gV[7]={0x11,0x11,0x11,0x11,0x11,0x0A,0x04};
    static const unsigned char gW[7]={0x11,0x11,0x11,0x15,0x15,0x1B,0x11};
    static const unsigned char gX[7]={0x11,0x11,0x0A,0x04,0x0A,0x11,0x11};
    static const unsigned char gY[7]={0x11,0x11,0x0A,0x04,0x04,0x04,0x04};
    static const unsigned char gZ[7]={0x1F,0x01,0x02,0x04,0x08,0x10,0x1F};
    static const unsigned char gAnd[7]={0x0E,0x11,0x10,0x0C,0x12,0x11,0x0F};
    static const unsigned char gMinus[7]={0,0,0,0x1F,0,0,0};
    static const unsigned char gDot[7]={0,0,0,0,0,0x0C,0x0C};
    static const unsigned char gSlash[7]={0x01,0x02,0x02,0x04,0x08,0x08,0x10};
    static const unsigned char gColon[7]={0,0x0C,0x0C,0,0x0C,0x0C,0};
    static const unsigned char gSpace[7]={0,0,0,0,0,0,0};
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
    int cx=x, row, col;
    for(; *s; s++){
        const unsigned char* g=glyph(*s);
        for(row=0; row<7; row++){
            unsigned char bits=g[row];
            for(col=0; col<5; col++) if(bits&(1<<col)) px(fb, cx+col, y+row, c);
        }
        cx += 6;
    }
}

/* ===== 原创像素宠物 (右下角轮播) ===== */
typedef struct { const char* name; int lv; const char* rows[8]; } Pet;
static const Pet pets[4] = {
    { "FLAMIT", 18, { "..RRRR..",".RORROR.",".RRRRRR.",".RKWWKR.",".RKKKKR.","..RRRR..","...RR...","...RR..." } },
    { "WAVICLAW",20,{ "..BBBB..",".BDBBDB.",".BBBBBB.",".BWBBWB.",".BKKKKB.","..BKKB..","..BKKB..","..BKKB.." } },
    { "THORN", 16, { "...GG...","..GLGL..",".GGGGGG.",".GKWWKG.",".GGKKGG.",".LGKKGL.",".L.LL.L.","..L..L.." } },
    { "SPARK", 17, { ".YY..YY.",".YY..YY.",".YYYYYY.",".YEWWEY.",".YYKKYY.",".YYYYYY.","..YYYY..","...YY..." } }
};
static void drawPet(u16* fb,int x,int y,const Pet* p){
    int i,j;
    for(j=0;j<8;j++){
        for(i=0;i<8;i++){
            char ch=p->rows[j][i];
            u16 c;
            if(ch=='.') continue;
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

enum View { V_GAME, V_SET, V_MENU, V_HELP };
static int curView=V_GAME;
static int curSel=0;
static int petIdx=0;
static int frame=0;

/* ===== 上屏：游戏视图 ===== */
static void drawCover(u16* fb,int x,int y,int w,int h,int idx){
    u16 base = (idx%2)? RGB15(6,18,26) : RGB15(24,10,8);
    int r, cx=x+w/2, cy=y+h/2;
    fillRect(fb,x,y,w,h,base);
    for(r=4; r<24; r+=4){
        frameRect(fb, cx-r, cy-r, r*2+1, r*2+1, 1, (r%8)? C_BLUE : C_RED);
    }
    drawText(fb, x+6, y+h-10, "COVER", C_TXT);
}
static void renderTopGame(void){
    int i;
    frameRect(topFb,4,20,92,120,2,C_RED);
    fillRect(topFb,5,21,90,118,C_PANEL);
    drawText(topFb,8,24,"GAME LIST",C_RED);
    for(i=0;i<NGAME;i++){
        int yy=36+i*14;
        if(i==curSel){ fillRect(topFb,7,yy-1,86,12,C_RED); drawText(topFb,10,yy,games[i].name,C_TXT); }
        else drawText(topFb,10,yy,games[i].name,C_MUTE);
    }
    { int covX=102, covY=20, covW=150, covH=96;
      frameRect(topFb,covX,covY,covW,covH,1,C_LINE);
      drawCover(topFb,covX+1,covY+1,covW-2,covH-2,curSel);
      drawText(topFb,covX+4,covY+covH-9, games[curSel].name, C_TXT); }
    { int infX=102, infY=120, infW=150, infH=66;
      frameRect(topFb,infX,infY,infW,infH,2,C_BLUE);
      fillRect(topFb,infX+1,infY+1,infW-2,infH-2,C_PANEL);
      drawText(topFb,infX+6,infY+4,"INFO",C_BLUE);
      drawText(topFb,infX+6,infY+14,games[curSel].name,C_TXT);
      drawText(topFb,infX+6,infY+26,"TYPE: ",C_MUTE); drawText(topFb,infX+36,infY+26,games[curSel].type,C_ACC);
      drawText(topFb,infX+6,infY+36,"SIZE: ",C_MUTE); drawText(topFb,infX+36,infY+36,games[curSel].size,C_ACC);
      drawText(topFb,infX+6,infY+46,"DESC: ",C_MUTE); drawText(topFb,infX+36,infY+46,games[curSel].desc,C_ACC);
      drawText(topFb,infX+6,infY+56,"[A] LAUNCH",C_RED); }
}
static void renderTopSettings(void){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"SETTINGS",C_BLUE);
    drawText(topFb,14,44,"LANG   : CN",C_TXT);
    drawText(topFb,14,58,"BRIGHT : 5/10",C_TXT);
    drawText(topFb,14,72,"VOLUME : 80%",C_TXT);
    drawText(topFb,14,86,"THEME  : ZUKAN",C_TXT);
    drawText(topFb,14,100,"CLOCK  : 12:00",C_TXT);
    drawText(topFb,14,124,"(DEMO VALUES)",C_MUTE);
}
static void renderTopMenu(void){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"MAIN MENU",C_BLUE);
    drawText(topFb,14,46,"1 GAME",C_TXT);
    drawText(topFb,14,62,"2 SETTINGS",C_TXT);
    drawText(topFb,14,78,"3 MEDIA",C_TXT);
    drawText(topFb,14,94,"4 HELP",C_TXT);
}
static void renderTopHelp(void){
    frameRect(topFb,4,20,248,166,1,C_LINE);
    drawText(topFb,10,26,"HELP",C_BLUE);
    drawText(topFb,10,44,"UP/DOWN : SELECT",C_TXT);
    drawText(topFb,10,56,"A : ENTER GAME",C_TXT);
    drawText(topFb,10,68,"B : BACK",C_TXT);
    drawText(topFb,10,96,"ORIGINAL HOMEBREW",C_MUTE);
    drawText(topFb,10,106,"ZUKAN-STYLE R4 MENU",C_MUTE);
}
static void renderTop(void){
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
static void renderBottom(void){
    int i;
    fillRect(subFb,0,0,FB_W,FB_H,C_BG);
    drawText(subFb,4,2,"R4 MENU",C_TXT);
    { const char* labels[4]={"GAME","SET","MENU","HELP"};
      const char* subs[4]={"BROWSE","OPTIONS","OVERVIEW","USAGE"};
      for(i=0;i<4;i++){
        int x=(i%2)*124+6, y=16+(i/2)*44, w=118, h=38;
        int active = (curView==i);
        frameRect(subFb,x,y,w,h,1, active? C_ACC : C_LINE);
        fillRect(subFb,x+1,y+1,w-2,h-2, active? C_PANEL2 : C_PANEL);
        drawText(subFb,x+6,y+6,labels[i], active? C_ACC : C_TXT);
        drawText(subFb,x+6,y+18,subs[i],C_MUTE);
      } }
    { int px0=178, py0=112;
      frameRect(subFb,px0-2,py0-2,74,74,1,C_LINE);
      fillRect(subFb,px0-1,py0-1,72,72,C_PANEL);
      drawPet(subFb,px0+4,py0+6,&pets[petIdx]);
      drawText(subFb,px0+4,py0+58, pets[petIdx].name, C_TXT);
      drawText(subFb,px0+4,py0+66, "LV", C_MUTE); }
    drawText(subFb,6,106,"PET CAROUSEL",C_MUTE);
    drawText(subFb,6,132,"1/4 AUTO",C_MUTE);
}

int main(void){
    int i;
    /* 电源：点亮 LCD + 两个 2D 引擎 */
    REG_POWERCNT = 0x000F;
    /* 显示模式 5(2D位图) + 启用 BG2 */
    REG_DISPCNT      = 0x0405;
    REG_DISPCNT_SUB  = 0x0405;
    /* 显存：bank A→上屏位图@0x06000000，bank C→下屏位图@0x06200000 */
    REG_VRAMCNT_A    = 0x80;
    REG_VRAMCNT_C    = 0x82;
    /* BG2 位图：16bpp、256x256、屏幕基址0 */
    REG_BG2CNT       = 0x0080;
    REG_BG2CNT_SUB   = 0x0080;

    /* 首次渲染 */
    renderTop();
    renderBottom();

    for(;;){
        /* 粗略垂直同步：等 VCOUNT 过可见区(进垂直消隐) */
        while(REG_VCOUNT < 160) { }
        /* 读按键：0=pressed */
        u16 keys = (~REG_KEYINPUT) & 0x03FF;
        if(keys & (1<<6)) curSel = (curSel+NGAME-1)%NGAME;   /* UP   */
        if(keys & (1<<7)) curSel = (curSel+1)%NGAME;         /* DOWN */
        if(keys & (1<<0)) curView = V_GAME;                  /* A    */
        if(keys & (1<<1)) curView = V_GAME;                  /* B    */
        if(keys & (1<<2)) curView = V_SET;                   /* X    */
        if(keys & (1<<3)) curView = V_MENU;                  /* Y    */
        if(keys & (1<<4)) curView = V_HELP;                  /* L    */
        /* 宠物轮播：约每 60 帧换一只 */
        frame++;
        if(frame>=60){ frame=0; petIdx=(petIdx+1)%4; }
        renderTop();
        renderBottom();
    }
    return 0;
}
