import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import { createVuestic } from 'vuestic-ui'

const app = createApp(App)
app.use(
  createVuestic({
    config: {
      colors: {
        variables: {
          primary: '#217a00',
          button: '#000'
        }
      }
    }
  })
)
app.use(router)

app.mount('#app')
