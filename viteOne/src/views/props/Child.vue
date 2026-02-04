<template>
    <p>{{ props.msg }}</p>
</template>

<script setup lang="ts">
let props = defineProps<{
    msg: string
}>()

</script>