<template>
  <div class="son1">
    <h1>我是子组件曹植</h1>
  </div>

</template>

<script setup lang="ts">
import bus from '@/bus'
import { onMounted } from 'vue';

onMounted(()=>{
    // 监听 car 信号
    bus.on('car',(car)=>{
         console.log(car);
    })
});
</script>

<style scoped> 
.son1{
  background-color: antiquewhite;
  width: 400px;
  height: 400px;
}
</style>