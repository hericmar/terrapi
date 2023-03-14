import { defineStore } from 'pinia'
import api from "@/api";

export const useMainStore = defineStore({
  id: 'mainStore',

  state: () => ({
    isLoggedIn: false
  }),

  actions: {
    async login(password: string): Promise<boolean> {
      return api.login(password)
        .then(async (isPasswordOk) => {
          if (isPasswordOk) {
            this.isLoggedIn = true;
            api.setPassword(password)

            return true;
          } else {
            this.isLoggedIn = false;

            return false;
          }
        })
        .catch(async (err) => {
          this.isLoggedIn = false;

          return false;
        })
    },
    logout() {
      this.isLoggedIn = false
      api.setPassword("")
    }
  }
})
