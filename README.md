# R4 黑白图鉴菜单启动器（云编译）

一个模仿《宝可梦黑白2》图鉴界面的 R4 烧录卡游戏菜单 UI，编译成 `.nds` 放进 R4 卡运行。

- **上屏**：左侧红框=游戏目录 | 右侧=游戏封面 | 下方蓝框=游戏信息
- **下屏**：4 格 R4 菜单(游戏/设置/菜单/帮助) + 右下角宠物精灵轮播
- 代码与素材均为**原创**，仅致敬黑白图鉴的布局风格。

## 自动编译（不用在自己电脑装 devkitPro）

本项目已配置 GitHub Actions。推送到 GitHub 后会自动：
1. 在云端安装 devkitPro(NDS 工具链)
2. 编译生成 `r4zukan.nds`
3. 把成品放在该次构建记录里，可直接下载

## 使用步骤

1. 把本仓库推送到你的 GitHub（见下方「如何推上去」）。
2. 打开仓库的 **Actions** 标签页 → 会看到自动触发的构建在跑。
3. 构建成功后（绿色对勾），点击该次构建 → 底部 **Artifacts** → 下载 `r4zukan.nds`。
4. 把 `r4zukan.nds` 复制到 R4 卡的 TF 卡里（放根目录即可）。

## 本地编译（可选，需自己装 devkitPro）

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
make
```
