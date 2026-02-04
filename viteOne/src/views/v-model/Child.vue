<template>
    <div class="son1">
        <h1>钱数  {{props.modelValue}}</h1>
        <button @click="handel">父子子键同步数据</button>

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
  width: 800px;
  height: 100px;
  background-color: aqua;
}
</style>