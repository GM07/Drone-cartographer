import Vue from 'vue';
import App from './app.vue';
import {ROUTER} from './router';
import vuetify from './plugins/vuetify';
import {STORE} from './store';
import VueSocketIO from 'vue-socket.io';
import SocketIO from 'socket.io-client';
import {
  SERVER_ADDRESS,
  GET_MISSION_STATUS_PATH,
} from './communication/server_constants';

Vue.config.productionTip = false;
Vue.use(
  new VueSocketIO({
    debug: false,
    connection: SocketIO(SERVER_ADDRESS, {
      path: GET_MISSION_STATUS_PATH,
    }),
    vuex: {
      store: STORE,
      actionPrefix: 'socket_',
    },
  })
);

new Vue({
  router: ROUTER,
  vuetify,
  store: STORE,
  render: h => h(App),
}).$mount('#app');
