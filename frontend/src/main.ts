import Vue from 'vue';
import App from './app.vue';
import {ROUTER} from './router';
import vuetify from './plugins/vuetify';
import {STORE} from './store';
import VueSocketIO from 'vue-socket.io';
import {SOCKETIO_SERVER_STATUS} from './communication/server_constants';

Vue.config.productionTip = false;
Vue.use(
  new VueSocketIO({
    debug: false,
    connection: SOCKETIO_SERVER_STATUS,
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
