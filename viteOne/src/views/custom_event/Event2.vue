<template>
    <div class="event2">
        <h1> 我是子组件二 </h1>
        <button @click="handle"> 点击一 </button>
        <br>
        <button @click="$emit('click','123456')">点击二</button>
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
  width: 400px;
  height: 400px;
}

</style>