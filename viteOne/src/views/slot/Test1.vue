<template>
<p> 我是子组件默然插槽 </p>
<slot> </slot>
<p> 我是子组件默然插槽 </p>
<slot name="a"></slot>
<p> 我是子组件默认插槽 </p>
<slot name="b"> </slot>
<p> 我是子组件默认插槽 </p>
</template>

<script setup lang="ts">
</script>