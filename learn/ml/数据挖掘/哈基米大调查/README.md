# 基于bilibili哈基米音乐视频的用户画像分析与分类研究
随着短视频与弹幕文化的兴起，Bilibili 评论区已成为年轻人情感表达与兴趣互动的重要场域。结合孙晓彤等人对弹幕评论影响的实证分析与4chan泛帖的数据采集方法，本文聚焦哈基米音乐热门视频评论，收集66,324条互动记录，构建用户情感与行为数据集。通过中文NLP与聚类算法，挖掘用户情感倾向、兴趣主题分布及活跃度分级，为个性化推荐、舆情监测和内容优化提供数据支撑。研究不仅丰富了弹幕用户画像理论，也为短视频平台社区治理与精准营销提供实践参考。

## 数据
总评论数：66324
![评论数量统计图.png](output/%E8%AF%84%E8%AE%BA%E6%95%B0%E9%87%8F%E7%BB%9F%E8%AE%A1%E5%9B%BE.png)
### CSV表头
| 序号 | 上级评论ID | 评论ID | 用户ID | 用户名 | 用户等级 | 性别 | 评论内容 | 评论时间 | 回复数 | 点赞数 | 个性签名 | IP属地 | 是否是大会员 | 头像 |
|------|-------------|--------|--------|--------|----------|------|----------|----------|--------|--------|----------|--------|--------------|------|
