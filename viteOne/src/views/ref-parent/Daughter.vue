<template>
    <div class="daughter">
        <h1> Daughter: {{money}} </h1>
        <el-button @click="handel($parent)">加钱</el-button>
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
.daughter {
    background-color: lightpink;
    flex: 1;
    width: 500px;
    display: flex;
    flex-direction: column;
    align-items: center;
    h1 {
        font-size: 24px;
    }
    .el-button {
        margin: auto;
    }
}   
</style>