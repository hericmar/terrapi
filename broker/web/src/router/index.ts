// Composables
import {createRouter, createWebHistory, RouteLocationNormalized} from 'vue-router'
import Vue from 'vue'

const routes = [
  {
    path: '/',
    component: () => import('@/layouts/default/Default.vue'),
    children: [
      {
        path: '',
        name: 'Home',
        // route level code-splitting
        // this generates a separate chunk (about.[hash].js) for this route
        // which is lazy-loaded when the route is visited.
        component: () => import(/* webpackChunkName: "home" */ '@/views/Home.vue'),
      },
      {
        path: '/environment/:clientId/:date?',
        name: 'Environment',
        component: () => import('@/views/Environment.vue')
      }
    ],
  },
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
  strict: true
})

export const getTitle = (route: RouteLocationNormalized, more?: string): string => {
  const baseTitle = 'TerraPi'
  const title = `${baseTitle} - ${<string>route.name || 'Home'}`
  if (more) {
    return `${title} - ${more}`
  }

  return title
}

router.afterEach((to, from) => {
  document.title = getTitle(to)
})

export default router
