<template>
    <div>
        <p> {{home}} </p>
        <p> {{car}} </p>
    </div>
</template>

<script setup lang="ts">

import { inject } from 'vue'
let props = defineProps<{
    home:string
}>();

let car = inject('take');

</script>