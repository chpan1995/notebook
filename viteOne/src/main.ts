import { createApp } from 'vue'
import '@/styles/index.scss'
// 入口文件main.ts全局安装element-plus,element-plus默认支持语言英语设置为中文
import ElementPlus from 'element-plus';
import 'element-plus/dist/index.css'
import zhCn from "element-plus/es/locale/lang/zh-cn"
// 报红原因： typescript 只能理解 .ts 文件，无法理解 .vue文件 解决办法见 tsconfig.app.json
import App from './App.vue'

// 引入路由
import router from './router'
// 创建仓库
import store  from '@/store';
// 路由守卫
import '../permission.ts'
const app = createApp(App);
app.use(
    ElementPlus,
    {
        locale:zhCn
    }
)
// 组件挂载到DOM
app.use(router);
app.use(store);
app.mount('#app');