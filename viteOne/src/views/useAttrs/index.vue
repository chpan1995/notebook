<template>
    <div>
        <h1> userAttrs </h1>
        <el-button type="primary" size="small"> 1111 </el-button>
        <HintButton type="primary" size="large" title="编辑" @click="handel" @xxx="handel2"></HintButton>
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

</style>