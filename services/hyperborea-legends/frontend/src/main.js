import './assets/main.css'

import {createApp} from 'vue'
import App from './App.vue'
import router from './router'
import 'vuestic-ui/css'
import { createVuestic} from 'vuestic-ui'

const app = createApp(App)


app.use(router)
app.use(createVuestic({
        config: {
            colors: {
                variables: {
                    primary: "rgba(255,255,255, 1)",
                    custom: "#A35600",
                },
            },
        },
    }
))
app.mount('#app')
