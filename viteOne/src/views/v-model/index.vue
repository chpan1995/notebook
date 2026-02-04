<template>
    <div>
        <h1> v-model {{ money }} {{ pageNo }} {{ pageSize }}</h1>
        <input type="text" v-model="info">
        <!-- <Child :money="money" @update:moneyValue="handel"></Child> -->
        <!--- v-model后面不写默认属性是modelValue---->
        <Child1 v-model="money"></Child1>
        <Child2 v-model:page-no="pageNo" v-model:page-size="pageSize"></Child2>
    </div>



</template>

<script setup lang="ts">

import { ref } from 'vue';
import Child1 from './Child.vue';
import Child2 from './Child2.vue';

let info = ref('');
let money = ref(10000);
let pageNo = ref(1);
let pageSize = ref(2);

let handel = (data: any) => {
    console.log(data);
    money.value = data;
}


</script>

<style scoped lang="scss"></style>