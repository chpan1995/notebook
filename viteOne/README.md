# Vue 3 + TypeScript + Vite

### 创建项目 
1. npm create vite@latest
- 会生成几个文件(tsconfig.json,tsconfig.app.json，tsconfig.node.json，vite.config.ts)
- 包含关系 tsconfig.json 包含 tsconfig.app.json 和 tsconfig.node.json
- tsconfig.node.json 包含 vite.config.ts

2. 安装 sass,element-plus,pinia,axios,vue-router
```
    # 凡是 main.ts / main.js、组件里 import 的库 → dependencies 凡是只给打包器用的 → devDependencies（-D）
    npm i -D sass , sass-loader
    npm i element-plus
    npm i pinia
    npm i axios
    npm i vue-router
```
3. 入口文件main.ts全局安装element-plus,element-plus默认支持语言英语设置为中文

4. 别名的配置
- src别名的配置
```
// vite.config.ts
import {defineConfig} from 'vite'
import vue from '@vitejs/plugin-vue'
import path from 'path'
export default defineConfig({
    plugins: [vue()],
    resolve: {
        alias: {
            "@": path.resolve("./src") // 相对路径别名配置，使用 @ 代替 src
        }
    }
})
```
- TypeScript 编译配置

```
// tsconfig.json
{
  "compilerOptions": {
    --"baseUrl": "./", // 解析非相对模块的基地址，默认是当前目录
    --"paths": { //路径映射，相对于baseUrl
      "@/*": ["src/*"] 
    }
  }
}
```
- tsconfig.app.json 配置
```
"compilerOptions": {
    "baseUrl": "./",
    "paths": {
      "@/*": ["src/*"]
    }
}
```
5. 环境变量的配置
- 项目根目录分别添加 开发、生产和测试环境的文件！ (.env.development,.env.production,.env.test)
```
# 变量必须以 VITE_ 为前缀才能暴露给外部读取
NODE_ENV = 'development'
VITE_APP_TITLE = '运营平台'
VITE_APP_BASE_API = '/api'
 
 
NODE_ENV = 'production'
VITE_APP_TITLE = '运营平台'
VITE_APP_BASE_API = '/prod-api'
 
 
# 变量必须以 VITE_ 为前缀才能暴露给外部读取
NODE_ENV = 'test'
VITE_APP_TITLE = '运营平台'
VITE_APP_BASE_API = '/test-api'
```
- 配置运行命令：package.json

```
 "scripts": {
    "dev": "vite --open",
    "build:test": "vue-tsc && vite build --mode test",
    "build:pro": "vue-tsc && vite build --mode production",
    "preview": "vite preview"
  },
```
- 通过import.meta.env获取环境变量
```
//import.meta.env获取环境变量--任何组件都可以使用
console.log(import.meta.env)
// 比如后面的api接口会使用到这里的基础路径
```
6. 默认样式设置
- 创建src/styles，导入index.scss 和 reset.scss


7. 创建路由
- main.ts 导入
- App.vue 实例化

8. 创建仓库