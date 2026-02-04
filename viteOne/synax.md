## 一、Vue 的 `:` 的作用

在 Vue 里：

```vue
:prop="value"
```

* `:` 是 `v-bind` 的简写
* 表示 **动态绑定一个 JS 表达式**
* 不加 `:`，就是 **静态字符串**

举例：

```vue
<MyComp title="编辑" />   <!-- 静态字符串 "编辑" -->
<MyComp :title="btnTitle" /> <!-- btnTitle 是 JS 变量 -->
```

---

## 二、对比你的例子

```vue
<HintButton type="primary" :icon="Edit" size="small" title="编辑" @click="handel" @xxx="handel2"></HintButton>
```

这里关键在 `:icon="Edit"`：

1. 你加了 `:` → Vue 会去 JS 里找变量 `Edit` 的值
2. 如果不加 `:` → Vue 会把 `icon` 的值当作字符串 `"Edit"` 传给组件

---

### ⚡ 问题关键

* `ElButton` / `HintButton` 的 `icon` 通常是 **一个组件对象或图标变量**，比如：

```ts
import { Edit } from '@element-plus/icons-vue';
```

`Edit` 是 JS 变量（不是字符串 `"Edit"`）
所以你必须用 `:icon="Edit"`，否则：

```vue
<HintButton icon="Edit" />
```

会传入字符串 `"Edit"`，最终不会渲染图标。

---

## 三、什么时候可以不加冒号？

* 当你想传**固定字符串**时：

```vue
<HintButton title="编辑" /> <!-- 直接字符串 -->
```

* 当你传 **布尔值 / 数字 / 对象 / 变量 / 函数** 时 **必须加 `:`**：

```vue
<HintButton :disabled="isDisabled" />
<HintButton :icon="Edit" />
<HintButton :style="{ color: 'red' }" />
```

---

## 四、总结一句话记忆

| 用法              | 解释               |
| --------------- | ---------------- |
| `prop="hello"`  | 静态字符串 `"hello"`  |
| `:prop="hello"` | JS 变量 `hello` 的值 |
| `:prop="{a:1}"` | 对象字面量            |
| `:prop="true"`  | 布尔值              |

💡 你的 `:icon="Edit"` ✅ 对
如果写成 `icon="Edit"` ❌，图标不会显示，因为它是 JS 变量，不是字符串。

---

