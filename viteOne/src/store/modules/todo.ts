import { defineStore } from "pinia";
import { ref, computed } from 'vue';
// 组合式 API（Setup Store） 推荐
let useTodoStore = defineStore('todo', () => {
    let todos = ref<Array<Object>>([
        {
        id:1,
        title:'吃饭'
        },
        {
        id:2,
        title:'睡觉'
        },
        {
        id:3,
        title:'打豆豆'
        },
    ]);

    let arr = ref<Array<number>>([1,2,3,4,5]);

    const tt:any=10;

    const total = computed(()=>{
        return arr.value.reduce((prev, next) => {
            return prev + next;
        }, 0)
    });

    // 务必返回一个对象: 属性与方法可以提供给组件使用
    return {
        todos,
        arr,
        tt,
        total,
        updateTodo() {
            todos.value.push({ id: 4, title: '组合式API方法' });
        }
    }
});

export default useTodoStore;