// 通过vue-router 插件实现模板路由配置
import { createRouter,createWebHashHistory } from 'vue-router';
import { constantRoute } from './router';
// 创建路由实例
const router = createRouter({
    history:createWebHashHistory(),
    routes:constantRoute,
    scrollBehavior() {
    return {
      left: 0,
      top: 0,
    }
  }
});

// import default 不带{}导入
export default router;