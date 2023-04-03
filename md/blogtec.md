主要使用了如下 `javascript` 库

`markdown-it` 用于把 `markdown` 格式的文章转换为 `html` 格式
`katex` 用于把类 `latex` 语法的数学公式转换为 `html` 格式
`prism` 用于代码语法高亮
`jquery` 用于简化页面元素操作

在 `vscode` 安装 `MPE(markdown preview enhanced)` 插件，写好 markdown 文章后，上传到网站，然后修改一个 `json` 文件，添加该文章的名称，日期和对应 `markdown` 文件名就可以了，不便之处就是没有单个文章的绝对链接不能做引用。因为 `MPE` 也使用的 `markdown-it` 和 `katex`，在 `vscode` 中写好的文章应该是所见即所得。

主要原理是，用一个 `json` 文件存储所有文章的信息，打开网站首页的时候，通过 `ajax` 获取这个文件，并显示所有文章的列表。当点击文章名称时，再通过 `ajax` 获取该文章所对应的 `markdown` 文件，通过 `markdown-it` 和 `katex` 转换为 `html` 网页。

转换过程是，首先通过正则表达式提取保存并用占位符替换文章中的数学公式，替换用的占位字符串应该在文章中不会出现。然后使用 `window.markdownit().render` 函数做转换，对转换后的字符串搜索占位符，使用 `katex.renderToString` 函数转换之前保存的对应位置上的数学公式并替换。

可以在主页点击右键查看网页源代码。
