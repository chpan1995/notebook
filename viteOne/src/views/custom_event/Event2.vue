<template>
    <div class="event2">
        <h1 class="title"> 我是子组件二 </h1>
        <div>
            <el-button @click="handle">点击一</el-button>
            <el-button @click="$emit('click','123456')">点击二</el-button>
        </div>
        
    </div>
</template>

<script setup lang="js">
    let $emit = defineEmits(['xxx','click']);
    let handle = () => {
        $emit('xxx',123);
    }
</script>

<style lang="scss">
.event2{
  background-color: pink;
  width: 100%;
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  div {
    margin: auto;
  }
}

</style>