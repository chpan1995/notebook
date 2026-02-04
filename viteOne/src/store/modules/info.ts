import { defineStore } from "pinia";
// 选项式 API
let infoStore = defineStore('info',{
    state:()=>{
        return {
            count:99,
            arr:[1,2,3,4,5,6]
        }
    },
    actions: {
        updateNum(a:number,b:number) {
            return this.count+=a+b;
        }
    },
    getters: {
        total() {
            let res:number = this.arr.reduce((prev: number, next: number) => {
                return prev + next;
            }, 0);
            return res;
        }
    }
})

export default infoStore;