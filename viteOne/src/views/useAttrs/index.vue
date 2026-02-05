<template>
    <div class="parent">
        <h1> userAttrs </h1>
        <el-button type="primary" size="small"> 1111 </el-button>
        <HintButton type="primary" size="large" title="编辑" @click="handel" @xxx="handel2"> attr </HintButton>
    </div>
</template>

<script setup lang="ts">

import HintButton from './HintButton.vue';

let handel=()=>{
  console.log(111);
}

let handel2=()=>{
  alert(111);
}


</script>

<style scoped lang="scss">
.parent {
    display: flex; 
    flex-direction: column; 
    align-items: center;
    height: 100vh;
    h1 {
        text-align: center;
        font-size: 28px;
        font-weight: bold;
    }
    el-button, .el-button {
      min-width: 100px;
      min-height: 60px;
      // 上下20px间距 左右是0
      margin: 20px 0;
    }
}
</style>