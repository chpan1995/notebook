<template>
    <div class="child">
        <p> 我是子组件默然插槽 </p>
        <slot> </slot>
        <p> 我是子组件默然插槽 </p>
        <slot name="a"></slot>
        <p> 我是子组件默认插槽 </p>
        <slot name="b"> </slot>
        <p> 我是子组件默认插槽 </p>
    </div>

</template>

<script setup lang="ts">
</script>

<style scoped lang="scss">
.child {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    width: 100%;
    background-color: aquamarine;
}
</style>