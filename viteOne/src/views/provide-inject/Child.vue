<template>
    <div>
        <p>{{home}}</p>
        <GrandChild :home="props.home"></GrandChild>
    </div>
</template>

<script setup lang="ts">

import GrandChild from './GrandChild.vue';
let props=defineProps(['home']);

</script>

<style lang="scss">
// 不加 scoped 会影响到子组件
p {
    font-size: 24px;
}
</style>