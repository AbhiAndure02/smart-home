import React, { Component } from 'react'
import { StyleSheet, Text, View } from 'react-native'

export class Switch extends Component {
  render() {
    return (
        <View style={styles.screen}>
        <Text style={styles.text}>This is the Switch screen</Text>
      </View>
    )
  }
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
export default Switch
