<template>
    <div class="not-found">
        <h1>404</h1>
        <p>页面未找到</p>
    </div>
</template>

<script setup lang="ts">
</script>

<style lang="scss" scoped>
.not-found {
    height: 100vh;
    width: 100vw;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    background-color: #f5f5f5;

    h1 {
        font-size: 10rem;
        margin: 0;
        color: #333;
    }

    p {
        font-size: 1.5rem;
        color: #666;
    }
}

</style>