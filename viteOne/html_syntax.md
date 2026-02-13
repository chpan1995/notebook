# HTML
```
常见HTML标签
水平线标签 <hr /> 
换行标签 <br /> 
<div>  <span>
超链接 <a href="www.baidu.com">点击进入另外一个文件</a> 
img标签 <img src="2.jpg" alt="test">
列表标签 列表标签分为三种
无序列表<ul>，无序列表中的每一项是<li> 

<ul>
	<li>默认1</li>
	<li>默认2</li>
	<li>默认3</li>
</ul>

type="属性值"。属性值可以选： disc(实心原点，默认)，square(实心方点)，circle(空心圆)。注意：项目符号可以是图片，需要通过CSS设置<li>标记的背景图片来实现。
当然了，列表之间是可以嵌套的。我们来举个例子。代码：
  <ul>
	<li><b>北京市</b>
		<ul>
			<li>海淀区</li>
			<li>朝阳区</li>
			<li>东城区</li>

		</ul>
	</li>

	<li><b>广州市</b>
		<ul>
			<li>天河区</li>
			<li>越秀区</li>
		</ul>
	</li>
  </ul>
ul的儿子，只能是li。但是li是一个容器级标签，li里面什么都能放，甚至可以再放一个ul
 
有序列表<ol>，里面的每一项是<li>


定义列表<dl>
<dl>英文单词：definition list，没有属性。dl的子元素只能是dt和dd。

<dt>：definition title 列表的标题，这个标签是必须的
<dd>：definition description 列表的列表项，如果不需要它，可以不加
备注：dt、dd只能在dl里面；dl里面只能有dt、dd。

举例：
<dl>
	<dt>第一条</dt>
	<dd>你若是觉得你有实力和我玩，良辰不介意奉陪到底</dd>
	<dd>我会让你明白，我从不说空话</dd>
	<dd>我是本地的，我有一百种方式让你呆不下去；而你，无可奈何</dd>

	<dt>第二条</dt>
	<dd>良辰最喜欢对那些自认能力出众的人出手</dd>
	<dd>你可以继续我行我素，不过，你的日子不会很舒心</dd>
	<dd>你只要记住，我叫叶良辰</dd>
	<dd>不介意陪你玩玩</dd>
	<dd>良辰必有重谢</dd>
</dl>

<dl>
	<dt>购物指南</dt>
	<dd>
		<a href="#">购物流程</a>
		<a href="#">会员介绍</a>
		<a href="#">生活旅行/团购</a>
		<a href="#">常见问题</a>
		<a href="#">大家电</a>
		<a href="#">联系客服</a>
	</dd>
</dl>
<dl>
	<dt>配送方式</dt>
	<dd>
		<a href="#">上门自提</a>
		<a href="#">211限时达</a>
		<a href="#">配送服务查询</a>
		<a href="#">配送费收取标准</a>
		<a href="#">海外配送</a>
	</dd>
</dl>

	<form>
		姓名：<input value="呵呵" >逗比<br>
		昵称：<input value="哈哈" readonly=""><br>
		名字：<input type="text" value="name" disabled=""><br>
		密码：<input type="password" value="pwd" size="50"><br>
		性别：<input type="radio" name="gender" id="radio1" value="male" checked="">男
			  <input type="radio" name="gender" id="radio2" value="female" >女<br>
		爱好：<input type="checkbox" name="love" value="eat">吃饭
			  <input type="checkbox" name="love" value="sleep">睡觉
			  <input type="checkbox" name="love" value="bat">打豆豆
	</form>

<select>：下拉列表标签

<textarea>标签：多

HTML标签是分等级的，HTML将所有的标签分为两种：
文本级标签：p、span、a、b、i、u、em。文本级标签里只能放文字、图片、表单元素。（a标签里不能放a和input）
容器级标签：div、h系列、li、dt、dd。容器级标签里可以放置任何东西。

DOM 操作
document.querySelector("selector") 通过CSS选择器获取符合条件的第一个元素。
document.querySelectorAll("selector") 通过CSS选择器获取符合条件的所有元素，以类数组形式存在。
```