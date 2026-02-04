<template>
    <div class="son2">
        <button @click="$emit('update:pageNo',props.pageNo+1)">pageNo</button>
        <button @click="$emit('update:pageSize',props.pageSize+1)">pageSize</button>
    </div>
</template>

<script setup lang="ts">

let props = defineProps<{
    pageNo:number,
    pageSize:number
}>();

let $emit =defineEmits(['update:pageNo','update:pageSize']);

</script>


<style scoped>
.son2{
  background-color: antiquewhite;
  width: 800px;
  height: 100px;
}
</style>