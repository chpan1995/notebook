import router from '@/router'

router.beforeEach(async (to: any, from: any, next: any) => {
    // to: 即将要进入的目标 路由对象
    // from: 当前导航正要离开的路由
    // next: 一定要调用该方法来 resolve 这个钩子。执行效果依赖 next 方法的调用参数。
    console.log('全局前置守卫');
    // allow access to the layout route and any nested routes under it
    // startsWith 方法用于判断当前字符串是否以另外一个给定的子字符串开头，返回 true 或 false。
    if (to.path.startsWith('/layout')) {
        next();
    } else {
        next({ path: '/layout', query: { redirect: to.path } });
    }
}
);

//全局后置守卫
router.afterEach((to: any, from: any) => {
    // to and from are both route objects.
})