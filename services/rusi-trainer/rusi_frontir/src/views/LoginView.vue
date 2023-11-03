<template>
  <div class="container">
    <form class="card" @submit.prevent="loginUser">
      <div class="topster">
        <p>Получи дозволение от Перуна</p>
      </div>
      <div class="field">
        <input
          class="gate"
          id="username"
          type="text"
          placeholder="Запиши прозвище своё"
          v-model="username"
        /><label for="username" title="Username">Прозвище</label>
      </div>
      <div class="field">
        <input
          class="gate"
          id="password"
          type="password"
          placeholder="Тайнопись свою не запамятуй"
          v-model="password"
        /><label for="password" title="Password">Тайнопись</label>
      </div>
      <button class="slavic-button" type="submit" title="Login">Возойти</button>
      <div class="groster">
        <p class="error" v-if="error" title="Error">Недосмотр: {{ error }}</p>
        <p>
          Ещё не с Перуном?
          <router-link to="/register" class="link" title="Register">Приобщиться</router-link>
        </p>
      </div>
    </form>
  </div>
</template>

<script>
import { loginMy } from '@/api/api'

export default {
  name: 'LoginView',
  data() {
    return {
      username: '',
      password: '',
      error: ''
    }
  },
  methods: {
    loginUser() {
      if (!this.username) {
        this.error = 'Позабыл прозвище'
        return
      }
      if (!this.password) {
        this.error = 'Позабыл тайнопись'
        return
      }
      loginMy(this.username, this.password)
        .then((token) => {
          this.$emit('onlogin', token)
          this.$router.push('/')
        })
        .catch((error) => {
          console.log(error)
          this.error = error.message
        })
    }
  }
}
</script>

<style scoped>
.topster {
  font-size: 30px;
  margin-bottom: -40px;
}
</style>
