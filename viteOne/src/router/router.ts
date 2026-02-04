import type { RouteRecordRaw } from "vue-router";

export const constantRoute: RouteRecordRaw[] = [
    {
        path: '/layout',
        component: () => import('@/layout/index.vue'),
        name: 'Layout',
        redirect: '/layout/props',
        meta: {
            title: '',
            hidden: false,
            icon: '',
        },
        children: [
            {
                path: 'props',
                component: () => import('@/views/props/index.vue'),
                meta: {
                    title: 'Props传值',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'custom_event',
                component: () => import('@/views/custom_event/index.vue'),
                meta: {
                    title: '自定义事件',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'event_bus',
                component: () => import('@/views/event_bus/index.vue'),
                meta: {
                    title: '事件总线',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'v-model',
                component: () => import('@/views/v-model/index.vue'),
                meta: {
                    title: 'v-model',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'useAttrs',
                component: () => import('@/views/useAttrs/index.vue'),
                meta: {
                    title: 'useAttrs',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'ref-parent',
                component: () => import('@/views/ref-parent/index.vue'),
                meta: {
                    title: 'ref-parent',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'provide-inject',
                component: () => import('@/views/provide-inject/index.vue'),
                meta: {
                    title: 'provide-inject',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'pinia',
                component: () => import('@/views/pinia/index.vue'),
                meta: {
                    title: 'pinia',
                    hidden: false,
                    icon: '',
                },
            },
            {
                path: 'slot',
                component: () => import('@/views/slot/index.vue'),
                meta: {
                    title: 'slot',
                    hidden: false,
                    icon: '',
                },
            }
        ]
    },
    {
        path: '/404',
        component: () => import('@/views/404/index.vue'),
        name: '404',
        meta: {
            title: '',
            hidden: false,
            icon: '',
        },
    },
    {
        // 重定向
        path: '/:pathMatch(.*)*',
        redirect: '/404',
        name: 'any',
        meta: {
            title: '',
            hidden: false,
            icon: '',
        },
    }
]