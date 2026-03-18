import pandas as pd
from pyvis.network import Network
import os
import argparse

def visualize_network(nodes_csv, edges_csv, requests_csv, answer_csv, output_html="network.html"):
    base_path = os.path.dirname(__file__)
    nodes_csv = os.path.join(base_path, "..", "data_files", f"{nodes_csv}.csv")
    edges_csv = os.path.join(base_path, "..", "data_files", f"{edges_csv}.csv")
    requests_csv = os.path.join(base_path, "..", "data_files", f"{requests_csv}.csv")
    answer_csv = os.path.join(base_path, "..", "data_files", f"{answer_csv}.csv")

    nodes_df = pd.read_csv(nodes_csv)
    edges_df = pd.read_csv(edges_csv)
    requests_df = pd.read_csv(requests_csv)
    answer_df = pd.read_csv(answer_csv)

    net = Network(
        height="100vh", 
        width="100%", 
        directed=True,
        font_color="black"
    )
    
    solution_set = set(zip(answer_df['From'], answer_df['To']))

    for _, row in nodes_df.iterrows():
        id, is_server = int(row['NodeId']), bool(row['IsServer'])
        node_color = "#4db8ff" if is_server else "#ff6b6b"
        node_label = f"SERVER {id}" if is_server else f"EXCHANGE {id}"
        net.add_node(id, label=node_label, color=node_color, size=45)

    for index, row in edges_df.iterrows():
        src, dest, rate_limit, latency, cost = int(row['Source']), int(row['Dest']), int(row['RateLimit']), float(row['Latency']), int(row['LeaseCost'])
        
        if (src, dest) in solution_set:
            color = "#ff0066" 
            width = 6
        else:
            color = "#838383"
            width = 1

        title = (
            f"Edge ID: {index}\n"
            f"Source: {src}\n"
            f"Dest: {dest}\n"
            f"Rate Limit: {rate_limit}\n"
            f"Latency: {latency}ms\n"
            f"Lease Cost: £{cost}"
        )

        net.add_edge(src, dest, color=color, width=width, title=title)

    for _, row in requests_df.iterrows():
        src, dest = int(row['Server']), int(row['Exchange'])
        net.get_node(src)['color'] = "#00ff73" 
        net.get_node(dest)['color'] = "#ff00ff" 
    
    net.set_options("""
    {
      "nodes": {
        "font": { "color": "#ffffff", "size": 14 }
      },
      "edges": {
        "color": { "inherit": false },
        "smooth": { "type": "continuous" }
      },
      "physics": {
        "forceAtlas2Based": {
          "gravitationalConstant": -8000,
          "centralGravity": 0.01,
          "springLength": 200,
          "springConstant": 0.08,
          "avoidOverlap": 1
        },
        "solver": "forceAtlas2Based",
        "stabilization": {
          "enabled": true,
          "iterations": 1,
          "updateInterval": 25
        }
      }
    }
    """)

    net.write_html(output_html)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--nodes', help='file name for the list of nodes', default='test_nodes')
    parser.add_argument('-e', '--edges', help='file name for the list of edges', default='test_edges')
    parser.add_argument('-r', '--requests', help='file name for the list of requests', default='test_requests')
    parser.add_argument('-a', '--answer', help='file name for the optimal network configuration found', default='test_answer')
    
    args = parser.parse_args()
    visualize_network(
        nodes_csv=args.nodes,
        edges_csv=args.edges,
        requests_csv=args.requests,
        answer_csv=args.answer
    )