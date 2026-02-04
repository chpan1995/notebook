<template>
    <div>
        <h1> Dauther: {{money}} </h1>
        <button @click="handel($parent)">加钱</button>
    </div>
</template>

<script setup lang="ts">

import { ref } from 'vue';

let money = ref<number>(1000);

const handel=(parm:any)=>{
  // $parent 直接取
  parm.money-=100;
  money.value+=100;
}

</script>

<style lang="scss">

</style>