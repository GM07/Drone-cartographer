import Vue from 'vue';
import VueRouter, {RouteConfig} from 'vue-router';
import Home from '@/views/home.vue';
import Mission from '@/views/mission.vue';

Vue.use(VueRouter);

const ROUTES: Array<RouteConfig> = [
  {
    path: '/',
    name: 'Home',
    component: Home,
  },
  {
    path: '/Mission',
    name: 'Mission',
    component: Mission,
  },
  {
    path: '*',
    redirect: '/',
  },
];

export const ROUTER = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: ROUTES,
});
