<template>
    <div>
        <Child home="dabieshu"></Child>
    </div>
</template>

<script setup lang="ts">

import {ref,provide} from 'vue';
import Child from './Child.vue';


let car = ref<string>('LEXUS');

// parm1 key parm2 vaule 
provide('take',car);

</script>

<style scoped lang="scss">

</style>