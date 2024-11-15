import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createMaterialTopTabNavigator } from '@react-navigation/material-top-tabs';
import Switch from './components/Switch';
import Timer from './components/Timer';

// Create a Top Tab Navigator
const Tab = createMaterialTopTabNavigator();

// Dummy screens

export default function App() {
  return (
    <NavigationContainer>
      <Tab.Navigator
        initialRouteName="Switch"
        screenOptions={{
          tabBarActiveTintColor: 'white',
          tabBarIndicatorStyle: { backgroundColor: 'blue' },
          tabBarStyle: { backgroundColor: 'purple' },
        }}
      >
        <Tab.Screen name="Switch" component={Switch} />
        <Tab.Screen name="Timer" component={Timer} />
       
      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  text: {
    fontSize: 18,
  },
});
