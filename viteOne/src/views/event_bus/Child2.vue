<template>
  <div class="son2">
    <h2>我是子组件2:曹丕</h2>
    <button class="btn" @click="handel"> 点击发送 </button>
  </div>
</template>

<script setup lang="ts">

import bus from '@/bus'

const handel = () => {
    bus.emit('car',{'car':123});
}

</script>

<style scoped>
.son2{
  width: 400px;
  height: 400px;
  background-color: pink;
}
.btn{
  width: 100px;
  height: 40px;
}

</style>