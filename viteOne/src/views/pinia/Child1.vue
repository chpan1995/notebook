<template>

<div class="child">
    <h1>{{info.count}}-----{{info.total}}</h1>
    <el-button @click="updateCount">点击我修改仓库数据</el-button>
</div>

</template>

<script setup lang="ts">
import useInfoStore from '@/store/modules/info'
let info = useInfoStore();
console.log(info);
//修改数据方法
const updateCount = () => {
  //仓库调用自身的方法去修改仓库的数据
  info.updateNum(66,77);
};
</script>

<style lang="scss">
.child {
  h1 {
    text-align: center;
    font-size: 24px;
  }
  .el-button {
    display: block;
    margin: 60px auto;
  }
  width: 100%;
  flex: 1;
  background: yellowgreen;
}
</style>