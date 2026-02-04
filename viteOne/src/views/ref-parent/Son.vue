<template>
    <div class="son">
        <h1>son money {{ money }} </h1>
    </div>
</template>

<script setup lang="ts">

import { ref } from 'vue';

let money = ref<number>(1000);

const fun=()=>{
    console.log("son");
}

defineExpose({
    money,
    fun
});

</script>


<style>
.son{
  background-color: antiquewhite;
  width: 500px;
  height: 500px;
}

</style>