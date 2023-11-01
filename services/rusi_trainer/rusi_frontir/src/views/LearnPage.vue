<template>
  <div class="learn-it">
    <form class="searcher" action="/learn-udar" method="get">
      <input
        class="gate"
        id="name"
        name="name"
        type="text"
        placeholder="наимение удара великого"
        required
      /><label for="name" title="Name">Наимение</label>
    </form>
    <h1 class="helper" v-if="udars.length > 0">Ты вложил в сборник:</h1>
    <div class="items">
      <div class="card" :key="udar.id" v-for="udar in udars">
        <h1 class="name">
          <router-link :to="{ path: '/learn-udar', query: { name: udar.name } }"
            >{{ udar.name }}}</router-link
          >
        </h1>
      </div>
    </div>
  </div>
</template>

<script>
import { getUdars } from '@/api/api'

export default {
  name: 'LearnPage',
  props: ['token'],
  data() {
    return {
      udars: []
    }
  },
  mounted() {
    getUdars(this.token)
      .then((udars) => (this.udars = udars))
      .catch((err) => {
        if (err.message === 'Auth error') {
          this.$emit('onlogin', null)
        }
        console.log(err)
      })
  }
}
</script>

<style scoped>
.learn-it {
  width: 100%;
}
.searcher {
  position: relative;
}
.searcher input {
  width: 100%;
  text-align: center;
  border: 7px solid white;
  padding-top: 1px;
  border-image: url('@/assets/border.png') 200 round round;
}
.items {
  display: flex;
  justify-content: center;
  flex-wrap: wrap;
  gap: 10px;
  padding: 20px;
}
.name {
  color: white;
  text-shadow: black 2px 2px;
  top: 0;
  margin-left: auto;
  font-size: 40px;
  right: 0;
  left: 0;
  margin-right: auto;
}
.helper {
  text-align: center;
  font-size: 40px;
  margin-top: 60px;
  text-shadow: white 2px 2px;
}
</style>
