<template>

<div class="child">
    <h1>{{info.count}}-----{{info.total}}</h1>
    <button @click="updateCount">点击我修改仓库数据</button>
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
  width: 200px;
  height: 200px;
  background: yellowgreen;
}
</style>