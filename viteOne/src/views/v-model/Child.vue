<template>
    <div class="son1">
        <h1>钱数  {{props.modelValue}}</h1>
        <el-button class="btn" @click="handel">父子子键同步数据</el-button>
    </div>
</template>

<script setup lang="ts">

let props = defineProps<{
    modelValue: number
}>()

let $emit = defineEmits(['update:modelValue']);
  let handel=()=>{
    $emit('update:modelValue',props.modelValue+10);
  }

</script>

<style>
.son1{
  flex: 1;
  width: 100%;
  background-color: aqua;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  h1 {
    text-align: center;
  }
  .btn {
    margin: auto;
  }
}
</style>