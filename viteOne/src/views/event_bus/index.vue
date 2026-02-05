<template>
  <div class="box">
    <h1>全局事件总线</h1>
    <hr>
    <div class="container">
      <Child1></Child1>
      <Child2></Child2>
    </div>
  </div>
</template>

<script setup lang="ts">
import Child1 from './Child1.vue';
import Child2 from './Child2.vue';
</script>

<style scoped lang="scss">
.box {
  width: 100vw;
  height: 100vh;
  background: yellowgreen;
  background: yellowgreen;
  .container{
    display: flex;
    height: 100%;
    gap: 60px;
    justify-content: center;
    align-items: center;
  }
  h1 {
    text-align: center;
  }
}
</style>