<template>
    <div>
        <Child1></Child1>
        <Child2></Child2>
    </div>
</template>

<script setup lang="ts">
//pinia:集中式管理状态容器,可以实现任意组件之间通信！！！
//核心概念:state、actions、getters
//pinia写法:选择器API、组合式API

import Child1 from './Child1.vue';
import Child2 from './Child2.vue';


</script>

<style scoped lang="scss">

</style>