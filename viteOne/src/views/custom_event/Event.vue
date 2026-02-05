<template>
    <div class="event1">
        <h1> 我是事件一 </h1>
        <el-button class="event1-btn" @click="$emit('click')"> 点我也执行 </el-button>
    </div>
</template>

<style scoped>
.event1{
  flex:1;
  width: 100%;
  background-color: skyblue;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  h1 {
    text-align: center;
  }
  .event1-btn {
    margin: auto;
  }
}
</style>