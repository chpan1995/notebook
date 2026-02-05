<template>
    <div class="main_container">
        <Child msg="Hello from Parent Component!" />
    </div>
    
</template>

<script setup lang="ts">
import Child  from './Child.vue';
</script>

<style scoped lang="scss">
.main_container {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    width: 100vw;
}
</style>