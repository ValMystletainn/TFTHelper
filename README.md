

# TFTHelper 云顶概率计算器

帮助你计算云顶中各种抽卡情况的概率计算器。现支持在给定等级和外部情况，使用“卡牌d”的策略时：
1. 给定抽卡次数，抽出目标卡数量的分布
2. 按抽足给定张数目标卡才停止抽卡的策略，所需抽卡次数的分布

## 安装与使用
### 普通版本
[下载](https://github.com/MapleHock/TFTHelper/releases/tag/0.1)，支持64位Windows7及以上系统。下载解压后用记事本打开./setting文件夹下的setting.ini文件，写入"tfthelper_zh_CN.qm"，指明所需加载的翻译文件，然后启动即可。

使用时选择感兴趣的输出模式，输入相关参数即可计算。
![image](https://github.com/MapleHock/TFTHelper/blob/master/doc/figure/runtimeZh.png)

### 命令行工具版本
下载./cli目录下的python脚本直接使用，或下载cpp文件自行编译后使用。


其支持的指令有
1. ```tfthelper -h```，或，```tfthelper --help```， 帮助页面，获得简单的帮助文档
2. ```tfthelper -r```， 或 ```tfthelper --rtable```，获取官方的单次d牌概率表
3. ```tfthelper -d <等级> <目标卡费用> <d牌次数> [已经取出的目标卡数] [已经取出的与目标卡同费用的其他卡数量]```  ， 打印d牌概率分布和相关统计量。[已经取出的目标卡数] [已经取出的与目标卡同费用的其他卡数量] 为可选参数，如不输入默认值为0
4. ```tfthelper -s <等级> <目标卡费用> <停止抽卡所需目标卡数> [已经取出的目标卡数] [已经取出的与目标卡同费用的其他卡数量]```  ， 打印抽卡次数概率分布和相关统计量。[已经取出的目标卡数] [已经取出的与目标卡同费用的其他卡数量] 为可选参数，如不输入默认值为0

## 杂项
对于算法原理请看：./doc/草稿.pdf



# TFTHelper - a prob calculator for TFT

This is an application that help you handle the probability of each situation in TeamFightTactics. It can do the math in those cases(given the level and other parameters, rolling without putting target tier card back policy):
1. For given rolling times, what's the distribution of the number of target card you get?
2. In the "roll until get x target cards" policy, what's the distribution of the rolling times you need?


## Installation and Usage

### normal version

[Download](https://github.com/MapleHock/TFTHelper/releases/tag/0.1)，only for 64bits Windows7/8/10 system now.
![image](https://github.com/MapleHock/TFTHelper/blob/master/doc/figure/runtimeEn.jpg)

Switch to the output random variable you want, fill the input parameters and run.

### CLI version
Download the files in ./cli. There are python script and cpp source code for you. The are the same cli programme. The python script can be directly used, while the cpp one show be complied by yourself first.

The subcommands are listed below:

1. ```tfthelper -h or tfthelper --help```, help page.
2. ```tfthelper -r``` or ```tfthelper --rtable```, to get the rolling prob table.
3. ```tfthelper -d <Level> <Tier> <#rolling> [#target drawn] [#same tier drawn(excluded #target drawn)]```, to get the distribution and statistics of how many the target cards you get.[#target drawn] [#same tier drawn(excluded #target drawn)] are optional argumentation with default value 0.
4. ```tfthelper -s <Level> <Tier> <#target> [#target drawn] [#same tier drawn(excluded #target drawn)]```, to get the distribution and statistics of  rolling times in "roll until get x target cards" policy. [#target drawn] [#same tier drawn(excluded #target drawn)] are optional argumentation with default value 0.

## misc
For how it works, see ./doc/draft_en.pdf.