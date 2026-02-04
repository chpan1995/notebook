<template>
    <!-- 把 父组件传进来的 title 用在 div 上 -->
    <div :title="$arrt.title" @click="$emit('xxx')">
        <!-- 这是 Vue 3 的语法糖 ，等价于： <el-button v-bind="$arrt"></el-button> -->
        <!-- 把所有 attrs 原封不动透传给 el-button -->
        <el-button :="$arrt"></el-button>
    </div>
</template>

<script setup lang="ts">

import { useAttrs } from 'vue';

let $arrt = useAttrs();
console.log($arrt);

let $emit = defineEmits(['xxx']);

</script>