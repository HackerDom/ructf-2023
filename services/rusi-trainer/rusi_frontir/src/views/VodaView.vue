<template>
  <div class="container">
    <BlockView v-if="blocked" />
    <va-modal v-model="drinkAlready" hide-default-actions blur allow-body-scroll no-dismiss>
      <template #header>
        <h1 class="helper-title">Ты испил водицы байкальской!</h1>
      </template>
      <div class="texter">Ты набрался сил и готов дальше громить ящеров.</div>
      <div class="texter">
        В байкале осталось <span class="left"> {{ water }} </span> литров воды
      </div>
      <template #footer>
        <router-link class="slavic-button" title="Go battle" to="/"> За новыми приёмами! </router-link>
      </template>
    </va-modal>
    <h1 class="helper-title" title="Rest and drink some water">Молодец, отдохни, да испей водицы</h1>
    <h1 class="helper" title="">Откуда будишь черпать воду?</h1>
    <form class="field" @submit.prevent="drink">
      <input
        class="gate"
        id="name"
        name="name"
        type="text"
        placeholder="Откда исьёшь водицу?"
        required
        v-model="from"
      /><label for="name" title="Water from?">Вода из</label>
    </form>
  </div>
</template>

<script>
import { drinkWater } from '@/api/api'
import BlockView from '@/views/BlockView.vue'

export default {
  name: 'VodaView',
  components: { BlockView },
  data() {
    return {
      from: '',
      blocked: false,
      drinkAlready: false,
      water: 0
    }
  },
  methods: {
    drink() {
      if (this.blocked || this.drinkAlready) {
        return
      }
      if (this.from.toLowerCase() === 'байкал' || this.from.toLowerCase() === 'байкала') {
        this.from = 'baikal'
      }
      drinkWater(this.from.toLowerCase())
        .then((left) => {
          this.drinkAlready = true
          this.from = ''
          this.water = left
        })
        .catch((err) => {
          console.log(err)
          this.blocked = true
        })
    }
  }
}
</script>

<style scoped>
.field {
  position: relative;
}
.container {
  flex-direction: column;
}
.helper-title {
  font-size: 40px;
  text-shadow:
    1px 1px 2px gold,
    0 0 1em white,
    0 0 0.2em green;
}
.helper {
  font-size: 30px;
  text-shadow:
    1px 1px 2px orange,
    0 0 1em black,
    0 0 0.2em darkred;
}
.texter {
  font-size: 30px;
}
.left {
  text-shadow:
    1px 1px 2px orange,
    0 0 1em black,
    0 0 0.2em darkred;
}
</style>
